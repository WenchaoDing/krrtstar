% Solving for P in AP + PA^T + MM^T - PC^T(NN^T)^-1CP = 0 where matlab uses
% the syntax A^TX + XA - XBB^TX + Q = 0
% A = dynamics matrix
% C = observation matrix
% N = observation noise matrix
% B*transpose(B) = transpose(C)*inv(N*transpose(N))*C
%       => B = transpose(C)*inv(tranpose(N))
% M = motion model noise matrix
% Q = M*transpose(M)

%% Select system and setup basic variables
clear
system = 'dblint1d'

% Set dimensionality information for system
if strcmp(system, 'snglint2d')
    % Single integrator
    X_DIM = 2
    U_DIM = 1
    Z_DIM = 1
elseif strcmp(system, 'dblint1d')
    % Double integrator 1D
    X_DIM = 2
    U_DIM = 1
    Z_DIM = 1
end

% Setup space for system
A = zeros(X_DIM, X_DIM);
B = zeros(X_DIM, U_DIM);
C = zeros(Z_DIM, X_DIM);
M = eye(X_DIM, X_DIM);
N = eye(Z_DIM, Z_DIM);

%% Setup system
if strcmp(system, 'snglint2d')
    error('System not setup')
elseif strcmp(system, 'dblint1d')
    A(1, 2) = 1
    B(2, 1) = 1
    C(1, 1) = 1
    C(2, 2) = 1
end

%% Solve CARE
[P, L, G] = care(A, transpose(C)/transpose(N), M*transpose(M));
P

%% Calculate K
K = P*transpose(C)*inv(N*transpose(N))

%% Solve LQR problem
% L can be calculated using (infinite horizon) LQR function matlab
% Can start with the penalty matrices Q = I, R = I to start (these are not
% the same Q as used in the paper draft.

Q_penalty = eye(X_DIM, X_DIM);
N_penalty = zeros(X_DIM, U_DIM);
R_penalty = eye(U_DIM, U_DIM);
[L, S, e] = lqr(A, B, Q_penalty, R_penalty, N_penalty);
L

%% Setup combined system
BL = B*L;
KC = K*C;
KN = K*N;
F = [A -BL; KC A - BL - KC]
G = [M zeros(X_DIM, size(KN, 2)); zeros(size(KN, 1), X_DIM) KN]

%% Solve Lyapunov
% Solving for \Sigma in F\Sigma + \SigmaF^T + GG^T where matlab uses the
% syntax AX + XA^T + Q
% A = F = combined natural dynamics as a function of L and K
% G = combined noise models as a function of K

Q = G*transpose(G)
Sigma = lyap(F, Q)

%%
X = [eye(X_DIM, X_DIM) zeros(X_DIM, X_DIM)]
U = [zeros(U_DIM, X_DIM) -L]
x_sigma = X*Sigma*transpose(X)
u_sigma = U*Sigma*transpose(U)

%%
R = ones(U_DIM, U_DIM) % kRRT* penalty matrix

k = 1 + trace(R*u_sigma)

R_tilde = R/k

final = inv(chol(x_sigma, 'lower'))